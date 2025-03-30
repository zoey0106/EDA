#!/bin/bash

# Set the variables
homeworkName="HW2"
binaryName="hw2"
timeLimit="180"
testcasePool=("public1" "public2" "public3" "public4" "public5" "public6")

# Set the directories and paths
root="$(pwd)"
outputDir="$root/output"
studentDir="$root/student"
testcaseDir="$root/testcase"
verifyBin="$root/verifier/verify"
chmod u+x "$verifyBin"
csvFile="$root/${homeworkName}_grade.csv"

# Define the execute function for running tests
function execute() {
    if [[ ! -f "$binaryName" ]]; then
        echo "N/A"
    else
        local log
        log="$(timeout "$timeLimit" time -p "./$binaryName" "$testcaseDir/$1.txt" "$outputDir/$1.out" 2>&1 >/dev/null)"
        if [[ "$log" =~ "real " && "$log" =~ "user " && "$log" =~ "sys " ]]; then
            echo "$(echo "$log" | grep "real" | tail -n 1 | cut -d ' ' -f 2)"
        else
            echo "TLE"
        fi
    fi
}

# Define the verify function for result verification
function verify() {
    if [[ ! -s "$outputDir/$1.out" ]]; then
        echo "The output file of $1 is not found or is empty."
    else
        local log
        log="$("$verifyBin" "$testcaseDir/$1.txt" "$outputDir/$1.out")"
        if [[ "$log" =~ "[Error]" ]]; then
            echo "There is an error in the output results of $1 ($(echo "$log" | grep "[Error]" | tail -n 1))."
        elif [[ "$log" =~ "[Success]" ]]; then
            echo "success"
        else
            echo "Something goes wrong when verifying the output results of $1."
        fi
    fi
}

echo "+------------------------------------------------+"
echo "|                                                |"
echo "|    This script is used for PDA $homeworkName grading.    |"
echo "|                                                |"
echo "+------------------------------------------------+"
echo "host name: $HOST"
echo "compiler version: $(g++ --version | grep "g++" | head -n 1)"
echo ""

# Set CSV title
csvTitle="\"student id\",\"tar.gz\",\"file structure\",\"readme\",\"makefile\",\"make clean\",\"make\""
for testcase in "${testcasePool[@]}"; do
    csvTitle="$csvTitle,\"$testcase cut size\",\"$testcase runtime\""
done
echo "$csvTitle,\"status\"" >"$csvFile"

# Traverse the student directory for grading
cd "$studentDir"
for student in *; do
    if [[ -d "$student" ]]; then
        cd "$student"
        studentId="$(echo "$student" | cut -d ' ' -f 1)"
        echo "grading on $studentId:"
        csvContent="\"$studentId\""

        # Check various requirements
        correctTarGz="no"
        correctStruct="no"
        haveReadme="no"
        haveMakefile="no"
        correctMakeClean="no"
        correctMake="no"
        if [[ -f "CS6135_${homeworkName}_${studentId}.tar.gz" ]]; then
            correctTarGz="yes"
            find . -mindepth 1 -maxdepth 1 -type d -exec rm -rf {} +
            tar -zxf "CS6135_${homeworkName}_${studentId}.tar.gz"
            if [[ -d "$homeworkName/src" && -d "$homeworkName/bin" ]]; then
                correctStruct="yes"
                cd "$homeworkName/src"
                if [[ $(find . -maxdepth 1 -type f -iname "README*" | wc -l) -eq 1 ]]; then
                    haveReadme="yes"
                fi
                if [[ $(find . -maxdepth 1 -type f -iname "Makefile" | wc -l) -eq 1 ]]; then
                    haveMakefile="yes"
                    if [[ ! -f "../bin/$binaryName" ]]; then
                        touch "../bin/$binaryName"
                    fi
                    make clean >/dev/null
                    if [[ ! -f "../bin/$binaryName" ]]; then
                        correctMakeClean="yes"
                    else
                        rm -f "../bin/$binaryName"
                    fi
                    make >/dev/null
                    if [[ -f "../bin/$binaryName" ]]; then
                        correctMake="yes"
                    fi
                fi
                cd "../bin"
            fi
        fi
        echo " checking item          | status"
        echo "------------------------|--------"
        echo " correct tar.gz         | $correctTarGz"
        echo " correct file structure | $correctStruct"
        echo " have README            | $haveReadme"
        echo " have Makefile          | $haveMakefile"
        echo " correct make clean     | $correctMakeClean"
        echo " correct make           | $correctMake"
        echo ""
        csvContent="$csvContent,\"$correctTarGz\",\"$correctStruct\",\"$haveReadme\""
        csvContent="$csvContent,\"$haveMakefile\",\"$correctMakeClean\",\"$correctMake\""

        # Verify the program by each testcase
        rm -rf "$outputDir"/*
        printf "%10s | %10s | %10s | %s\n" "testcase" "cut size" "runtime" "status"
        echo "-----------|------------|------------|--------"
        statusArray=()
        for testcase in "${testcasePool[@]}"; do
            printf "%10s | " "$testcase"
            cutSize="N/A"
            runtime="$(execute "$testcase")"
            if [[ "$runtime" == "N/A" ]]; then
                status="The executable file ($binaryName) is not found."
                statusArray+=("$status")
            elif [[ "$runtime" == "TLE" ]]; then
                status="Time out while testing $testcase."
                statusArray+=("$status")
            else
                status="$(verify "$testcase")"
                if [[ "$status" == "success" ]]; then
                    cutSize="$(grep CutSize "$outputDir/$testcase.out" | cut -d ' ' -f 2)"
                else
                    runtime="N/A"
                    statusArray+=("$status")
                fi
            fi
            printf "%10s | %10s | %s\n" "$cutSize" "$runtime" "$status"
            csvContent="$csvContent,$cutSize,$runtime"
        done
        echo "$csvContent,\"${statusArray[@]}\"" >>"$csvFile"

        cd "$studentDir"
    fi
done

echo "+------------------------------------------------+"
echo "|                                                |"
echo "|   Successfully write grades to ${homeworkName}_grade.csv   |"
echo "|                                                |"
echo "+------------------------------------------------+"
