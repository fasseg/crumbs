#!/bin/bash

if [[ $OSTYPE == darwin* ]]; then
    FIND_CMD=gfind
else
    FIND_CMD=find
fi

if [[ -f '/etc/crumbs.conf' ]]; then
    DATA_PATH=$(cat /etc/crumbs.conf | grep path= | cut -d '=' -f 2)
    
    if [[ $DATA_PATH != "/"* ]]; then
        DATA_PATH="$HOME/$DATA_PATH"
    fi
else
    DATA_PATH="$HOME/.crumbs"
fi

_crumbs_completion() {
    local cur="${COMP_WORDS[COMP_CWORD]}"
    local actions=("list" "insert" "insert-exec" "show" "exec" "delete" "edit")
    
    COMPREPLY=()

    if [[ $COMP_CWORD -eq 1 ]]; then
        if [[ -z $cur ]];then
            COMPREPLY+=(${actions[@]})
        else 
            for action in "${actions[@]}";do
                if [[ $action =~ ^$cur ]];then 
                    COMPREPLY+=($action)
                fi
            done
        fi
    fi

    if [[ $COMP_CWORD -eq 2 ]]; then
        local prev=${COMP_WORDS[COMP_CWORD - 1]}
        if [[ $prev =~ ^(show|exec|delete|edit)$ ]];then
            if [[ -z $cur ]];then
                COMPREPLY+=( $($FIND_CMD $DATA_PATH -type f -printf "%P\n") )
            else
                COMPREPLY+=( $($FIND_CMD $DATA_PATH -type f -printf "%P\n" | grep "^$cur") )
            fi
        fi
    fi
}

complete -F _crumbs_completion crumbs
