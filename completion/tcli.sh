#/usr/bin/env bash

exe='tcli'
LIST_HEADER='Candidate nodes:'

log(){
    local content=$1
    echo "$content" >> /tmp/tcli_completion.log
}

_tcli_completion() {
    # log "=========="
    local temp_words="${COMP_WORDS[@]:0:$COMP_CWORD}"
    # log "temp_words: ${temp_words}"
    local exec="${COMP_WORDS[0]}"
    # log "exec: ${exec}"
    local args="${COMP_WORDS[@]:1:$COMP_CWORD-1}"
    # log "args: ${args}"
    local cmd=("${exec}" "--list" "--silence" "${args}")
    # log "cmd: ${cmd[*]}"
    # echo "cmd: ${cmd[@]}" >> /tmp/tcli_completion.log
    local output=$(${cmd[@]})
    # log "output: $output"

    local header=$(echo "${output}" | head -n 1)
    if [[ x"${header}" != x"${LIST_HEADER}" ]]; then
        COMPREPLY=()
        return 1
    fi
    local content=$(echo "${output}" | tail -n +2)
    COMPREPLY=($(compgen -W "$content" -- "$2"))
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
