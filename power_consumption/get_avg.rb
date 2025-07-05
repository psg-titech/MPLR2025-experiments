#!/usr/local/bin/ruby

cur_joule = 0
cur_time = 0
prev = 0
is_gps_acc = /gps_acc/.match?(ARGV[0])
File.foreach(ARGV[0]) do |l|
  row = l.split(',')
  simstate = row[2][0].to_i
  if simstate != prev then
    if prev == 0 then
      puts "#{cur_joule * 3.3 / 1000 / 100 / 1000 / 1000}, #{cur_time / 1000.0 / 100.0}"
    end
    # uA(/0.01ms) * 3.3V -> uJ(/0.01ms)
    # uJ(/0.01ms) / 1000 -> uJ(/0.01s)
    # uJ(/0.01s) / 100 -> uJ
    # uJ / 1000 / 1000 -> J
    prev = simstate
    cur_joule = 0
    cur_time = 0
  end
  cur_joule += row[1].to_f
  cur_time += 1
  if is_gps_acc then
    if cur_time >= 60 * 30 then
      puts "#{cur_joule * 3.3 / 1000 / 100 / 1000 / 1000}, #{cur_time / 1000.0 / 100.0}"
      break
    end
  end
end
