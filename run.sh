#!/bin/bash
NOMBRE=SO-TP2

sudo docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so-multi-platform:3.0

docker start $NOMBRE
docker exec -it $NOMBRE make clean -C/root/Toolchain
docker exec -it $NOMBRE make clean -C/root/
docker exec -it $NOMBRE make -C/root/Toolchain
docker exec -it $NOMBRE make -C/root/
docker stop $NOMBRE

# Set FLAG based on the input argument
FLAG=""
[[ "$1" = "-d" ]] && FLAG="-s -S -d int"

# Set common qemu options
QEMU_CMD="qemu-system-x86_64 $FLAG -hda Image/x64BareBonesImage.qcow2 -m 512"

if [[ "$(uname)" == "Linux" ]]; then
    # Linux specific execution
    sudo $QEMU_CMD
else
    # macOS specific execution
    export AUDIO_DRIVER="coreaudio"
    $QEMU_CMD -audiodev $AUDIO_DRIVER,id=audio0 -machine pcspk-audiodev=audio0
fi

# para cerrar:
docker rm $NOMBRE
