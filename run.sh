#!/bin/bash


echo "Player 1: $1"
echo "Player 2: $2"


function generate_command () {
    OPTION=$1
    case $OPTION in
    choice1)
    COMMAND="";
    ;;

    choice2)
    COMMAND="";
    ;;

    choiceN)
    COMMAND="";
    ;;

    *)
    esac
    return $COMMAND;
}

generate_command $1;
COMMAND1=$?;
generate_command $1;
COMMAND2=$?;

echo "Command 1: $COMMAND1"
echo "Command 2: $COMMAND2"





