#/usr/bin/env bash

exe='tcli'

_tcli_completion() {
    local temp_words="${COMP_WORDS[@]:0:$COMP_CWORD}"
    local output=$(${temp_words[@]} --list --silence)
    COMPREPLY=($(compgen -W "$output" -- "$2"))
}

getExecs() {
    local prefix=$1
    local execs=()
    execs+=($(compgen -c ${prefix}))
    for ((i = 0; i < ${#execs[@]}; i++)); do
        execs[i]=$(basename ${execs[$i]})
    done
    execs+=(${prefix})
    printf "%s\n" "${execs[@]}" | sort | uniq
}

readarray -t execs <<< "$(getExecs ${exe})"

for tcli in ${execs[@]}; do
    complete -o default -F _tcli_completion ${tcli}
done
