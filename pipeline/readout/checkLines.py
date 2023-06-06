with open(r"output.txt", "r") as fp:
    x = len(fp.readlines())

    if x > 12:
        print("Number of lines above 12!")
        return 1
    else:
        print("Number of linesOK!")
        return 0
