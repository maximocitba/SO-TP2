#!/bin/bash
NOMBRE=SO-TP2

sudo docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so-multi-platform:3.0

docker start $NOMBRE

# Parse arguments
USE_BUDDY=0
FLAG=""
for arg in "$@"; do
    if [[ "$arg" == "buddy" ]]; then
        USE_BUDDY=1
    elif [[ "$arg" == "-d" ]]; then
        FLAG="-s -S -d int"
    fi
done

if [[ $USE_BUDDY -eq 1 ]]; then
    echo "Compiling with buddy memory manager"
    docker exec -it $NOMBRE make clean -C/root/
    docker exec -it $NOMBRE make -C/root/ KERNEL_MEMMAN_IMPL=mymemman
else
    echo "Compiling with noob memory manager"
    docker exec -it $NOMBRE make clean -C/root/
    docker exec -it $NOMBRE make -C/root/
fi
docker stop $NOMBRE

QEMU_CMD="qemu-system-x86_64 $FLAG -hda Image/x64BareBonesImage.qcow2 -m 512"

if [[ "$(uname)" == "Linux" ]]; then
    sudo $QEMU_CMD
else
    export AUDIO_DRIVER="coreaudio"
    $QEMU_CMD -audiodev $AUDIO_DRIVER,id=audio0 -machine pcspk-audiodev=audio0
fi

docker rm $NOMBRE