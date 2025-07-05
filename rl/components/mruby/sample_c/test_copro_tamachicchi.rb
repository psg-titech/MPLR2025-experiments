#
# mrbc -Bmrbbuf test_copro_tamachicchi.rb
#

def tamagocchiSound()
    Ledc.freq 783# ソ
    Ledc.start
    Copro.delayMs 50
    Ledc.stop
    Copro.delayMs 100
    Ledc.freq 659# ミ
    Ledc.start
    Copro.delayMs 300
    Ledc.stop
    Copro.delayMs 100
    Ledc.freq 523# ド
    Ledc.start
    Copro.delayMs 100
    Ledc.freq 659# ミ
    Copro.delayMs 100
    Ledc.freq 1046# ド
    Copro.delayMs 100
    Ledc.stop
end

def blink(l, v)
    l.put v
    Copro.delayMs 300
    l.put(0, Lcd.blank)
    l.put(1, Lcd.blank)
    Copro.delayMs 300
    l.put v
    Copro.delayMs 100
    l.put(0, Lcd.blank)
    l.put(1, Lcd.blank)
    Copro.delayMs 300
    l.put v
    (1..15).each do
        Copro.delayMs 100
        l.put(0, Lcd.blank)
        l.put(1, Lcd.blank)
        Copro.delayMs 100
        l.put v
    end
end

def deatta(l, v, icon_v)
    l.put(0, v)
    Copro.delayMs 2000
    l.put(1, Lcd.nideatta)
    Copro.delayMs 4000
    l.put icon_v
end

def gattai(l, v, v_icon)
    l.put(0, Lcd.oyaoya)
    Copro.delayMs 1000
    l.put(1, Lcd.tamachicchitachiga)
    Copro.delayMs 2000
    l.put(0, Lcd.tamachicchitachiga)
    l.put(1, Lcd.blank)
    Copro.delayMs 1000
    l.put(1, Lcd.kagayakidashita)
    Copro.delayMs 3000
    blink(l, v_icon)
    Copro.delayMs 2000
    l.put(0, v)
    Copro.delayMs 1000
    l.put(1, Lcd.unite)
    Copro.delayMs 2000
end

Ledc.init 1
Ledc.duty 2
l = Lcd.init(12, 11, 0, 800000)
l.put(0, Lcd.blank)
l.put(1, Lcd.blank)
Copro.gpio_input(6, Copro.gpio_pulldown)
Copro.gpio_input(5, Copro.gpio_pulldown)
Copro.gpio_output 4

nextevent = true
state = -1
while true do
    if nextevent then
        Copro.gpio(4, false)
        state += 1
        tamagocchiSound()
        case state
            when 0 then
                blink(l, Lcd.icon_tamachicchi)
                Copro.delayMs 1000
                l.put(0, Lcd.tamachicchi)
                Copro.delayMs 1000
                l.put(1, Lcd.gaumareta)
                Copro.delayMs 3000
            when 1 then 
                deatta(l, Lcd.oookayamacchi, Lcd.icon_oookayamacchi)
            when 2 then
                deatta(l, Lcd.suzukakecchi, Lcd.icon_suzukakecchi)
            when 3 then
                gattai(l, Lcd.toukoudaicchi, Lcd.icon_toukoudaicchi)
            when 4 then
                deatta(l, Lcd.ikashikadaicchi, Lcd.icon_ikashikadaicchi)
            when 5 then
                gattai(l, Lcd.kagakudaicchi, Lcd.icon_kagakudaicchi)
        end
        nextevent = false
    end
    puts "AA"
    case state
    when 0 then
        l.put Lcd.icon_tamachicchi
    when 1 then
        l.put Lcd.icon_oookayamacchi
    when 2 then
        l.put Lcd.icon_suzukakecchi
    when 3 then
        l.put Lcd.icon_toukoudaicchi
    when 4 then 
        l.put Lcd.icon_ikashikadaicchi
    else
        l.put Lcd.icon_kagakudaicchi
    end
    puts "BB"
    switchbtn = false
    prevswitchbtn = false
    while(!prevswitchbtn || switchbtn) do
        prevswitchbtn = switchbtn
        switchbtn = Copro.gpio? 5
        Copro.delayMs(100)
    end
    puts "CC"
    nextevent = Copro.sleep_and_run do
        prevbtn = false
        prevbtn2 = false
        count = 0
        ev = false
        btn = false
        btn2 = false
        while (!prevbtn || btn) do
            btn2 = Copro.gpio? 6
            if prevbtn2 && !btn2 then
                count = count + 1
                ev = count > 5
            end
            Copro.gpio(4, ev)
            prevbtn = btn
            prevbtn2 = btn2
            btn = Copro.gpio? 5
        end
        ev
    end
end