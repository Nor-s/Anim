DIR="$( cd "$( dirname "$0" )" && pwd )"
if [ -f "$DIR/../python/bin/python3" ] ; then 
    echo python exist
else 

  cd "$DIR/../external/cpython"

  PKG_CONFIG_PATH="$(brew --prefix tcl-tk)/lib/pkgconfig" \
    ./configure --with-openssl=$(brew --prefix openssl@1.1) --prefix="$DIR/../python"  --enable-optimizations

  make

  make install
  cd "$DIR/../python/bin"
  
  ./pip3 install mediapipe-silicon
  ./pip3 install pyglm

# mediapipe-silicon

fi