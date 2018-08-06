#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

# This script can be used to compile all the tags and install under a given directory REST_Install_Path
# All the existing tags will be installed under REST_Install_Path/tagName
# It will also update REST_Install_Path/master with the latest update of master branch
#
# J. Galan - Javier.Galan.Lacarra@cern.ch
# 23 - Dec - 2016

import os,sys, time, commands
import subprocess, StringIO
import vars

def repairgit():
    out, err = subprocess.Popen(["git rev-parse --short HEAD"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
    if "fatal" in err:
        print "linking remote git repository to local"
        os.chdir(vars.opt["Source_Path"])
        os.system("rm -rf .git")
        os.system("git init")
        os.system("git remote add origin "+vars.var["REST_Repository"])
        os.system("git checkout -b "+vars.opt["Branch"])
        os.system("git fetch --depth 1")
        os.system("git reset --hard origin/"+vars.opt["Branch"])
        print "git repository is ready!"
        return True
    else :
        print "git repository is ready!"
        return False

def commitid():
    if os.path.exists(vars.opt["Source_Path"] + "/.git/"):
        os.chdir(vars.opt["Source_Path"])
        out, err = subprocess.Popen(["git rev-parse --short HEAD"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
        return str(out)
    return ""

def branchname():
    if os.path.exists(vars.opt["Source_Path"] + "/.git/"):
        os.chdir(vars.opt["Source_Path"])
        out, err = subprocess.Popen(["git branch"], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True).communicate()
        n1=str(out).find("*")
        s=str(out)[n1:-1]
        n2=s.find("\n")
        return str(out)[n1+2:n2+n1]
    return ""


def main():
    newgit=repairgit()
    if newgit==False:
        print "updating local git repository of REST"
        os.chdir(vars.opt["Source_Path"])
        os.system("git stash")
        p = subprocess.Popen(['git fetch origin '+vars.opt["Branch"]], stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        out, err = p.communicate()
        if "up-to-date" in out:
            print "REST is already up-to-date"
            return False
        elif "fatal:" in err:
            print err
            return False
        os.system("git reset --hard FETCH_HEAD")
        return True
    print "Project files has been updated!"
    print commitid()
    print branchname()
    return True


        
if __name__ == '__main__':
    print "Local changes will be overwritten! (except additions)"
    print "Make backup and do it carefully!"
    print "Press a key to continue"
    raw_input()
    main()


##===================================================
##================ Must be changed ==================
##===================================================
## Modify this PATH with your REST_v2 build directory
#REST_Build_Path = "/home/jgalan/git/REST_v2/build"

## Modify this PATH with the final destination of REST installation
#REST_Install_Path = "/programas/REST/"

## You might need to add some environment definitions needed to build REST
#os.environ["GARFIELD_HOME"]="/programas/garfield"
#os.environ["LD_LIBRARY_PATH"]= "/programas/root34.32/root/lib:/programas/geant4.10.02.p02-install/lib::/programas/garfield/lib"
#os.environ["PATH"]= os.environ["PATH"] + ":/programas/root34.32/root/bin/"
##===================================================

#os.chdir( REST_Build_Path )

## Updating master
#os.system ('/usr/bin/git checkout master' )

#p = subprocess.Popen(['/usr/bin/git', 'pull', '--tags'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

#out, err = p.communicate()

#if err == "" and "up-to-date" in out:
#	print "Master is already updated"
#else:
#	os.system( "cmake -DINSTALL_PREFIX=" + REST_Install_Path + "master" + " ../" )
#	os.system ('/usr/bin/make clean' )
#	os.system ('/usr/bin/make -j4' )
#	os.system ('/usr/bin/make install' )
#	os.environ["REST_PATH"] = REST_Install_Path  + "master"
#	os.chdir( REST_Build_Path + "/../packages/restG4/build/" )
#	os.system( "cmake ../" )
#	os.system ('/usr/bin/make clean' )
#	os.system ('/usr/bin/make -j4' )
#	os.system ('/usr/bin/make install' )


## Installing new tags

#p = subprocess.Popen(['/usr/bin/git', 'tag'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

#out, err = p.communicate()

#buf = StringIO.StringIO( out )

#gitTags = []
#length = 1
#while length > 0:
#	tag = buf.readline().rstrip()
#	length = len( tag )
#	if length > 0:
#		gitTags.append( tag )

#p = subprocess.Popen(['ls', REST_Install_Path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

#out, err = p.communicate()

#buf = StringIO.StringIO( out )

#installedTags = []

#length = 1
#while length > 0:
#	tag = buf.readline().rstrip()
#	length = len( tag )
#	if length > 0 and tag.find("master"):
#		installedTags.append( tag )

#notInstalledTags = []

#for x in gitTags:
#	alreadyInstalled = 0
#	for y in installedTags:
#		if x == y:
#			alreadyInstalled = 1

# 	if x.find("v2.0.0") == 0:
#		alreadyInstalled = 1
		
#	if not alreadyInstalled:
#		notInstalledTags.append( x )

#for tag in notInstalledTags:
#	os.system( "git checkout -b " + tag + " " + tag )
#	os.system( "cmake -DINSTALL_PREFIX=" + REST_Install_Path + tag + " ../" )
#	os.system ('/usr/bin/make clean' )
#	os.system ('/usr/bin/make -j4' )
#	os.system ('/usr/bin/make install' )
#	os.environ["REST_PATH"] = REST_Install_Path  + tag
#	os.chdir( REST_Build_Path + "/../packages/restG4/build/" )
#	os.system( "cmake ../" )
#	os.system ('/usr/bin/make clean' )
#	os.system ('/usr/bin/make -j4' )
#	os.system ('/usr/bin/make install' )
#	os.system ('git checkout master' )
#	os.system ('git branch -d ' + tag )
#	os.chdir( REST_Build_Path )

