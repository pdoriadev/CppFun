#!/usr/bin/env bash
printf "\n"
echo "'#!/bin/bash' is the shebang line. 'bash' says we're using bash scripting commands here."

printf "\n\n================ echo  ================\n"
# echo shebang
echo "'echo' command lets us output strings to console."
# echo description
printf "\nGenerally, avoid using echo. printf is more versatile and avoids some bash pitfalls (i.e. See #14 under \"https://mywiki.wooledge.org/BashPitfalls#read_.24foo\". Echo is for the simplest output to the console."
touch echoFile1.txt
printf "\nWe can write \"This is echo file 1.\" to echoFile1.txt by redirecting echo's output. \n\t(Redirection is a powerful technique that can be used with many commands, including printf.)"
echo -n -e "\nThis is echo file 1." > echoFile1.txt
printf "\nechoFile1: " && cat echoFile1.txt

printf "\n\nInfo for 'echo': ss64.com/bash/echo.html"
printf "\nInfo for 'printf': \'ss64.com/bash/printf.html\'\n"
printf "\n============ End of echo  ============="

printf "\n\n================ cat ================"
printf "\n'cat' is a versatile command for concatenating and outputing text files to console or to a file."

# Write to file by redirecting printf.
touch catFile1.txt
str="word"
printf "\n\nWe can write to catFile1.txt using printf. i.e. printf \"$str\" > catFile1.txt"
printf "%s" "$str" > catFile1.txt # Redirect output to catFile1.
printf "\ncatFile1\'s text: "
cat catFile1.txt # cat outputs the content of catFile1.txt to the console.

# Write to file with contents of another file.
touch catFile2.txt
printf "\n\nThen, we can use 'cat' to copy catFile1's text to catFile2."
cat catFile1.txt > catFile2.txt # The redirect operator stops cat from outputting catFile1's text to the console.
printf "\ncatFile1\'s text: " && cat catFile1.txt
printf "\ncatFile2\'s text: " && cat catFile2.txt

# Append to text file using printf.
shloopyStr="WORD"
printf "\n\n%s" "Let's append the string \"WORD\" to catFile2.txt."
printf "$shloopyStr" >> catFile2.txt # Redirect  output to catFile2.
printf "\ncatFile2's text: " && cat catFile2.txt

# Concatenate file contents and write the concatenation to another file.
touch catFile3.txt
printf "\n\nNow, let's concatenate the contents of catFile1 and catFile2 into catFile3."
cat catFile1.txt catFile2.txt > catFile3.txt
printf "\ncatFile3: " &&  cat catFile3.txt

# Write contents of all files with common extension to one file.
touch catAllTxtFiles.txt
printf "\n\nFinally, let's merge the content of all txt files into catAllTxtFiles using the .txt extension."
cat *.txt > catAllTxtFiles.txt 
printf "\ncatAllTxtFiles.txt: " && cat catAllTxtFiles.txt

printf "\n\nss64.com/cat.html for more info."
printf "\n========= End of cat =================\n"

printf "Info for command redirection: \'ss64.com/bash/syntax-redirection.html\'\n" 
