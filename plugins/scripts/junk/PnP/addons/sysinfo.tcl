
alias sysinfo {

    #GPU
    set graphics [split [exec system_profiler SPDisplaysDataType] \n]
    set type [lindex [split [lsearch -inline $graphics {*Type:*}] {:}] 1]
    set resolution [string map [list " " ""] [lindex [split [lsearch -inline $graphics {*Resolution:*}] {:}] 1]]
    set model [string trim [lindex [split [lsearch -inline -nocase $graphics {*Chipset Model*}] {:}] 1]]
    set depth [lindex [lindex [split [lsearch -inline -nocase $graphics {*Pixel Depth*}] {:}] 1] 0]
    
    # Hardware
    set hardware [split [exec system_profiler SPHardwareDataType] \n]
    set hmodel [string trim [lindex [split [lsearch -inline -nocase $hardware {*Model Name*}] {:}] 1]]
    set processor [string trim [lindex [split [lsearch -inline -nocase $hardware {*Processor Name*}] {:}] 1]]
    set speed [string trim [lindex [split [lsearch -inline -nocase $hardware {*Processor Speed*}] {:}] 1]]
    set memory [string trim [lindex [split [exec system_profiler SPHardwareDataType | grep Memory] {:}] 1]]
    
    #OS
    set os [split [exec system_profiler SPSoftwareDataType] \n]
    set operativeSystem [join [lrange [lindex [split [lsearch -nocase -inline $os {*System Version:*}] {:}] 1] 0 3]]
    set uptime [join [lrange [split [lsearch -inline -nocase $os {*Time since boot*}] {:}] 1 end] {:}]
    
    #HD
    set hd [split [exec system_profiler SPSerialATADataType] \n]
    set space [join [lrange [lindex [split [lsearch -nocase -inline $hd {*Capacity:*}] {:}] 1] 0 1]]
    set available [join [lrange [lindex [split [lsearch -nocase -inline $hd {*Available:*}] {:}] 1] 0 1]]

    /SAY \002$type\002:($model $resolution@$depth) \002$hmodel\002:($processor@$speed) \002Memory\002:($memory) \002OS\002:($operativeSystem,$uptime \002up\002) \002HD\002:($available/$space)
    complete
}