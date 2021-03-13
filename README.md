# qrtape

A utility for encoding a file into a series of file fragments for storage
within QR codes.

## build

Only a single dependency is needed to build qrtape: `TCLAP`. This can be
installed on most distros:

```
sudo apt-get install libtclap-dev
```

With TCLAP installed, follow the standard cmake build routine:

```
mkdir build
cd build
cmake ..
make -j`nproc`
```

## usage

qrtape permits encode and decode modes. Here are examples for both.

### encoding

Encoding splits a file into a number of .bin fragments. These are intended to
be encoded into a QR code.

```
qrtape --encode -s 2331 --input equalizer-12k-stereo-vbr.opus -p equalizer_
```

### decoding

Decoding reads QR codes from stdin and emits the binary contents to stdout.
This permits creating simple pipelines.

```
./qrtape/qrtape -d -s 2331 --allow-skip
```

## audio playback

The following is an example of how to encode an audio file to qrtape. These
commands are provided merely for reference.

### encoding

```
opusenc --discard-comments --discard-pictures \
    --framesize 60 --bitrate 12 \
    equalizer.flac equalizer-12k-stereo-vbr.opus
qrtape --encode -s 2331 \
    --input equalizer-12k-stereo-vbr.opus -p equalizer_
for i in {0..156}; do \
    qrencode -8 -m 0 -s 16 -r -L M \
    equalizer_$i.bin -o equalizer_$i.png; \
done
for i in {0..156}; do \
    echo printing $i; \
    sudo brother_ql -b pyusb -m QL-700 -p usb://0x04f9:0x2042 \
        print -l 62 --600dpi --no-cut equalizer_$i.png; \
    sleep 12; \
done
```

### decoding

```
zbarcam /dev/video0 --prescale=1920x1080 \
    --raw -Sdisable -Sqrcode.enable -Sbinary \
    | ./qrtape/qrtape -d -s 2331 --allow-skip \
    | tee equalizer.opus \
    | mplayer -
```
