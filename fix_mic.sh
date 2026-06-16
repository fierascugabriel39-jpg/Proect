#!/bin/bash

# 1. Re-activăm podul USB pentru AudioRelay
echo "> Resetăm conexiunea USB (ADB forward)..."
adb forward tcp:59100 tcp:59100

# 2. Recreăm modulele de sunet dacă au dispărut
echo "> Verificăm modulele de sunet virtuale..."

# Încercăm să încărcăm Sink-ul (Ieșirea)
pactl load-module module-null-sink sink_name=VirtualMic sink_properties=device.description="Telefon_Huawei_Mic" 2>/dev/null

# Încercăm să încărcăm Sursa (Microfonul)
pactl load-module module-remap-source source_name=HuaweiMicSource master=VirtualMic.monitor source_properties=device.description="Microfon_Huawei_USB" 2>/dev/null

# 3. Setăm noul microfon ca fiind cel implicit pentru sistem
echo "> Setez Microfon_Huawei_USB ca sursă implicită..."
pactl set-default-source HuaweiMicSource

# 4. MUTARE AUTOMATĂ: 
# Căutăm dacă AudioRelay rulează și îl mutăm forțat pe placa noastră virtuală
echo "> Mutăm automat fluxul AudioRelay pe microfonul virtual..."
INPUT_ID=$(pactl list sink-inputs short | grep -i "AudioRelay" | awk '{print $1}')
if [ ! -z "$INPUT_ID" ]; then
    pactl move-sink-input "$INPUT_ID" VirtualMic
    echo "> Succes: AudioRelay a fost direcționat către microfon!"
else
    echo "> Notă: AudioRelay nu pare să redea sunet în acest moment. Conectează-te din aplicație!"
fi

echo "--------------------------------------------------"
echo "GATA! Acum asigură-te că:"
echo "1. Aplicația de pe telefon e pe 'Server' și 'Microfon'."
echo "2. Ai apăsat 'CONNECT' în aplicația AudioRelay de pe PC."
echo "--------------------------------------------------"
