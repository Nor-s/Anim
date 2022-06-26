echo start sh---------------------------------------------------
DIR="$( cd "$( dirname "$0" )" && pwd )"
if [ -f "$DIR/dist/mp2mm/mp2mm" ] ; then 
    echo mp2mm exist
else 
    pyinstaller $DIR/mp2mm.spec  --noconfirm
fi
echo end sh---------------------------------------------------