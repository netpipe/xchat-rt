package require Tk

proc ui {} {
    wm geometry . 600x370
    grid rowconfigure . 0 -weight 0
    grid rowconfigure . 1 -weight 1
    grid rowconfigure . 2 -weight 1
    grid rowconfigure . 3 -weight 1
    grid rowconfigure . 4 -weight 1
    grid columnconfigure . 0 -weight 0
    grid columnconfigure . 1 -weight 1
    if {1} {
        
        ttk::treeview .l
        foreach id [list {Channel options} {DCC accept} Favorites Language Messages Protection Serverlist Other] {
            .l insert {} end -text $id
        }
        grid .l -column 0 -sticky nsw -pady 8 -padx 2
    } ;#Listbox
    if {1} {
        
        ttk::labelframe .lbl -text Test
        grid .lbl -column 1 -sticky nesw -row 0 -rowspan 4
    } ;# Labelframe
    
    if {1} {
        ttk::button .btnOK -text {OK} -command {exit}
        grid .btnOK -column 1 -sticky e -row 4 
    }
    
    wm deiconify .
}

wm protocol . WM_DELETE_WINDOW chk_exit

proc chk_exit {} {
    wm witdhraw .
}

update
#vwait ::forever
