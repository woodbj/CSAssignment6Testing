#!/bin/bash

# Clean up old test and output files before processing
rm -f *.tst *.cmp *.out *.asm

# Check if there are any .vm files in the current directory
shopt -s nullglob  # Ensure the glob does not return the literal string if no files are found
vm_files=(*.vm)

if [ ${#vm_files[@]} -eq 0 ]; then
    echo "No .vm files found in the current directory."
    exit 1
fi

# Process each .vm file
for vm_file in "${vm_files[@]}"
do
    base_name="${vm_file%.vm}"


    echo "--Testing $base_name--"
    TestBuilder "$vm_file"

    # echo "--Translating $base_name to asm--"
    VMTranslator "$vm_file"

    # echo "--Running tests--"
    # Loop through all corresponding .tst files for this base name
    for tst_file in "${base_name}"*.tst
    do
        test_name="${tst_file%.tst}"
        
        if [[ -e "$tst_file" && ! "$tst_file" == *".asm.tst" ]]; then  # Check if the .tst file exists and is not .asm.tst
            # echo "--Building compare file for $tst_file--"
            timeout 0.75 VMEmulator.sh "$tst_file" |

            # echo "--Testing ASM for $test_name--"
            CPUEmulator.sh "${test_name}.asm.tst"  # Adjust this if there are multiple asm files
        fi
    done
done
