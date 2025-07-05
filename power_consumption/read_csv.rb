#!/usr/local/bin/ruby

require 'csv'
cur_joule = 0
cur_time = 0
prev = 0
File.foreach(ARGV[0]) do |l|
  row = l.split(',')
  simstate = row[2].getbyte(1)
  if simstate != prev then
    puts "#{cur_joule * 3.3 / 1000 / 100 / 1000 / 1000}, #{cur_time / 1000.0 / 100.0}"
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
end
