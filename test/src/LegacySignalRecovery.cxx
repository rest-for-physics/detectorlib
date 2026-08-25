#include <TBasket.h>
#include <TBranch.h>
#include <TClass.h>
#include <TFile.h>
#include <TKey.h>
#include <TNamed.h>
#include <TRestDetectorSignalEvent.h>
#include <TTree.h>
#include <gtest/gtest.h>

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {
namespace fs = std::filesystem;

class TemporaryDirectory {
   public:
    TemporaryDirectory() {
        static std::atomic<unsigned long> sequence{0};
        path = fs::temp_directory_path() /
               ("rest_slim_recovery_test_" +
                std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()) + "_" +
                std::to_string(sequence++));
        fs::create_directories(path);
    }
    ~TemporaryDirectory() {
        std::error_code error;
        fs::remove_all(path, error);
    }
    fs::path path;
};

int RunProcess(const std::vector<std::string>& arguments, const char* recoveryMacro = nullptr) {
#ifdef _WIN32
    return -1;
#else
    const pid_t child = fork();
    if (child == 0) {
        if (recoveryMacro != nullptr) setenv("REST_LEGACY_RECOVERY_MACRO", recoveryMacro, 1);
        std::vector<char*> argv;
        for (const auto& argument : arguments) argv.push_back(const_cast<char*>(argument.c_str()));
        argv.push_back(nullptr);
        execv(argv.front(), argv.data());
        _exit(127);
    }
    if (child < 0) return -1;
    int status = 0;
    while (waitpid(child, &status, 0) < 0 && errno == EINTR) {
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128;
#endif
}

std::vector<char> ReadBytes(const fs::path& filename) {
    std::ifstream input(filename, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

std::vector<char> ReadBasketPayload(TFile& file, TBranch& branch, Int_t index) {
    auto* basket = branch.GetBasket(index);
    if (basket == nullptr) return {};
    const Int_t bytes = branch.GetBasketBytes()[index] - basket->GetKeylen();
    std::vector<char> result(bytes);
    if (bytes > 0 && file.ReadBuffer(result.data(), branch.GetBasketSeek(index) + basket->GetKeylen(), bytes))
        return {};
    return result;
}

void ExpectRawBranchShape(TFile& sourceFile, TBranch* source, TFile& recoveredFile, TBranch* recovered) {
    ASSERT_NE(source, nullptr);
    ASSERT_NE(recovered, nullptr);
    EXPECT_EQ(std::string(source->GetName()), recovered->GetName());
    EXPECT_EQ(source->GetEntries(), recovered->GetEntries());
    EXPECT_EQ(source->GetTotBytes(), recovered->GetTotBytes());
    EXPECT_EQ(source->GetZipBytes(), recovered->GetZipBytes());
    EXPECT_EQ(source->GetWriteBasket(), recovered->GetWriteBasket());
    for (int basket = 0; basket < source->GetWriteBasket(); ++basket) {
        EXPECT_EQ(source->GetBasketBytes()[basket], recovered->GetBasketBytes()[basket]);
        EXPECT_EQ(ReadBasketPayload(sourceFile, *source, basket),
                  ReadBasketPayload(recoveredFile, *recovered, basket));
    }
    ASSERT_EQ(source->GetListOfBranches()->GetEntries(), recovered->GetListOfBranches()->GetEntries());
    for (int index = 0; index <= source->GetListOfBranches()->GetLast(); ++index)
        ExpectRawBranchShape(sourceFile, static_cast<TBranch*>(source->GetListOfBranches()->At(index)),
                             recoveredFile, static_cast<TBranch*>(recovered->GetListOfBranches()->At(index)));
}

TEST(LegacySignalRecovery, ConvertsOnlySignalBranchAndPreservesOpaqueContent) {
#ifdef _WIN32
    GTEST_SKIP() << "The recovery command is currently POSIX-only";
#else
    TemporaryDirectory temporary;
    const fs::path source = temporary.path / "legacy.root";
    const fs::path output = temporary.path / "fixed.root";
    ASSERT_EQ(RunProcess({REST_LEGACY_FIXTURE_WRITER, source.string()}), 0);
    fs::permissions(source, fs::perms::owner_write, fs::perm_options::remove);
    const auto originalBytes = ReadBytes(source);

    // thisREST.sh aliases restRoot to `restRoot -l`; the recovery dispatch must
    // still recognize the command when ROOT launcher flags precede its option.
    ASSERT_EQ(RunProcess({REST_LEGACY_RESTROOT, "-l", "--recover-legacy-signals", source.string(), "--output",
                          output.string()},
                         REST_LEGACY_MACRO),
              0);
    EXPECT_EQ(ReadBytes(source), originalBytes);

    TFile original(source.c_str(), "READ");
    TFile fixed(output.c_str(), "READ");
    ASSERT_FALSE(original.IsZombie());
    ASSERT_FALSE(fixed.IsZombie());
    auto* oldTree = dynamic_cast<TTree*>(original.Get("EventTree"));
    auto* newTree = dynamic_cast<TTree*>(fixed.Get("EventTree"));
    ASSERT_NE(oldTree, nullptr);
    ASSERT_NE(newTree, nullptr);
    ASSERT_EQ(oldTree->GetEntries(), newTree->GetEntries());
    ASSERT_NE(oldTree->GetAlias("opaqueCode"), nullptr);
    EXPECT_STREQ(newTree->GetAlias("opaqueCode"), oldTree->GetAlias("opaqueCode"));
    auto* treeNote = dynamic_cast<TNamed*>(newTree->GetUserInfo()->FindObject("TreeNote"));
    ASSERT_NE(treeNote, nullptr);
    EXPECT_STREQ(treeNote->GetTitle(), "must survive fast cloning");

    auto* oldOpaque = oldTree->GetBranch("LegacyOpaqueEventBranch");
    auto* newOpaque = newTree->GetBranch("LegacyOpaqueEventBranch");
    ExpectRawBranchShape(original, oldOpaque, fixed, newOpaque);
    auto* opaqueClass = TClass::GetClass("LegacyOpaqueEvent");
    ASSERT_NE(opaqueClass, nullptr);
    EXPECT_FALSE(opaqueClass->HasDictionary());

    // StreamerInfo makes an unavailable top-level class visible as a non-null
    // emulated TClass.  This is deliberately not sufficient reason to call
    // TKey::ReadObj(): real legacy REST metadata can crash there when an
    // abstract compiled base cannot be instantiated.  The recovery must leave
    // such a key opaque in the byte-copied candidate.
    auto* opaqueMetadataClass = TClass::GetClass("LegacyOpaqueMetadata");
    ASSERT_NE(opaqueMetadataClass, nullptr);
    EXPECT_FALSE(opaqueMetadataClass->HasDictionary());

    for (const char* keyName : {"LegacyOpaqueMetadata", "AnalysisTree"}) {
        auto* oldKey = original.GetKey(keyName);
        auto* newKey = fixed.GetKey(keyName);
        ASSERT_NE(oldKey, nullptr);
        ASSERT_NE(newKey, nullptr);
        EXPECT_STREQ(oldKey->GetClassName(), newKey->GetClassName());
        EXPECT_EQ(oldKey->GetSeekKey(), newKey->GetSeekKey());
        EXPECT_EQ(oldKey->GetNbytes(), newKey->GetNbytes());
    }

    auto* signalEvent = new TRestDetectorSignalEvent;
    auto* signalBranch = newTree->GetBranch("TRestDetectorSignalEventBranch");
    ASSERT_NE(signalBranch, nullptr);
    ASSERT_GE(newTree->SetBranchAddress("TRestDetectorSignalEventBranch", &signalEvent), TTree::kMatch);
    for (Int_t entry = 0; entry < 3; ++entry) {
        ASSERT_GT(signalBranch->GetEntry(entry), 0);
        EXPECT_EQ(signalEvent->GetRunOrigin(), 12);
        EXPECT_EQ(signalEvent->GetSubRunOrigin(), 2);
        EXPECT_EQ(signalEvent->GetID(), 100 + entry);
        EXPECT_EQ(signalEvent->GetSubID(), entry);
        EXPECT_EQ(signalEvent->GetSubEventTag(), TString::Format("tag-%d", entry));
        EXPECT_EQ(signalEvent->GetTimeStamp().GetSec(), 1000 + entry);
        EXPECT_EQ(signalEvent->GetTimeStamp().GetNanoSec(), 200 + entry);
        EXPECT_EQ(signalEvent->isOk(), entry != 1);
        ASSERT_EQ(signalEvent->GetNumberOfSignals(), 2);
        for (Int_t signalIndex = 0; signalIndex < 2; ++signalIndex) {
            auto* signal = signalEvent->GetSignal(signalIndex);
            EXPECT_EQ(signal->GetSignalID(), 10 * entry + signalIndex);
            EXPECT_EQ(signal->GetSignalName(), "signal-" + std::to_string(signalIndex));
            EXPECT_EQ(signal->GetSignalType(), "legacy");
            ASSERT_EQ(signal->GetNumberOfPoints(), 2);
            for (Int_t point = 0; point < 2; ++point) {
                const Float_t expectedTime = entry + 0.25F * signalIndex + 0.1F * point;
                const Float_t expectedCharge = 100 * entry + 10 * signalIndex + point;
                EXPECT_EQ(signal->GetTime(point), static_cast<Double_t>(expectedTime));
                EXPECT_EQ(signal->GetData(point), static_cast<Double_t>(expectedCharge));
            }
        }
    }
    EXPECT_NE(fixed.Get("REST_LegacySignalRecovery"), nullptr);

    const fs::path secondOutput = temporary.path / "must-not-exist.root";
    EXPECT_NE(RunProcess({REST_LEGACY_RESTROOT, "--recover-legacy-signals", output.string(), "--output",
                          secondOutput.string()},
                         REST_LEGACY_MACRO),
              0);
    EXPECT_FALSE(fs::exists(secondOutput));
#endif
}

TEST(LegacySignalRecovery, ExistingOutputIsNeverTouched) {
#ifdef _WIN32
    GTEST_SKIP() << "The recovery command is currently POSIX-only";
#else
    TemporaryDirectory temporary;
    const fs::path source = temporary.path / "legacy.root";
    const fs::path output = temporary.path / "existing.root";
    ASSERT_EQ(RunProcess({REST_LEGACY_FIXTURE_WRITER, source.string()}), 0);
    {
        std::ofstream sentinel(output);
        sentinel << "keep me";
    }
    const auto before = ReadBytes(output);
    EXPECT_NE(RunProcess({REST_LEGACY_RESTROOT, "--recover-legacy-signals", source.string(), "--output",
                          output.string()},
                         REST_LEGACY_MACRO),
              0);
    EXPECT_EQ(ReadBytes(output), before);
#endif
}

TEST(LegacySignalRecovery, RejectsMismatchedIntermediate) {
#ifdef _WIN32
    GTEST_SKIP() << "The recovery command is currently POSIX-only";
#else
    TemporaryDirectory temporary;
    const fs::path source = temporary.path / "legacy.root";
    const fs::path output = temporary.path / "must-not-exist.root";
    ASSERT_EQ(RunProcess({REST_LEGACY_FIXTURE_WRITER, source.string()}), 0);
    const auto originalBytes = ReadBytes(source);
    EXPECT_NE(RunProcess({REST_LEGACY_RESTROOT, "--recover-legacy-signals", source.string(), "--output",
                          output.string()},
                         REST_LEGACY_WRONG_INTERMEDIATE),
              0);
    EXPECT_EQ(ReadBytes(source), originalBytes);
    EXPECT_FALSE(fs::exists(output));
#endif
}

}  // namespace
