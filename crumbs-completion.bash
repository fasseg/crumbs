#!/bin/bash

_crumbs_completion() {
    local path=$(cat /etc/crumbs.conf | grep path= | cut -d '=' -f 2)
    if [[ $path != "/"* ]];then
        path="$HOME/$path"
    fi

    local actions=("list" "insert" "insert-exec" "show" "exec" "delete")
    local cur="${COMP_WORDS[COMP_CWORD]}"

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
        if [[ $prev =~ ^(show|exec|delete)$ ]];then
            if [[ -z $cur ]];then
                COMPREPLY+=( $(find $path -type f -printf "%P\n") )
            else
                COMPREPLY+=( $(find $path -type f -printf "%P\n" | grep "^$cur") )
            fi
        fi
    fi
}

complete -F _crumbs_completion crumbs
