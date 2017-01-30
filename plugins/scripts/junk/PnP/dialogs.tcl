#
package require Tk
proc dxkicks {} {
    wm geometry . 560x100
    
    ttk::combobox .c -width 460
    pack .c
    ttk::button .k -text {Kick} -command [list puts mierda]
    pack .k -anchor e
    set file $::filekicks/kicks.txt
    set fs [open $file]
    set buffer [split [read $fs] \n]
    close $fs
    .c configure -values $buffer
}

proc bug {} {
    
    wm geometry . 560x300
    ttk::frame .f
    pack .f -expand 1 -fill both
    ttk::label .f.lbl1 -text {Se ha trazado un error y es posible depurarlo.}
    pack .f.lbl1
    text .f.text -width 560 -height 50
    pack .f.text
    ttk::button .f.btnSend -text Enviar -command {bugSend}
    pack .f.btnSend
}

proc dialog {window} {
    eval $window
}
proc addon {} {
    
    global pnp
    tkCleanUp
    wm geometry . 400x440
    wm title . {PnP Addons}
    wm resizable . 0 0
    ttk::labelframe .lbl1 -text [lang TkInstalledAddon] -width 280 -height 180
    listbox .lbl1.list -width 30
    pack .lbl1 -fill x -padx 5
    grid .lbl1.list -column 0 -rowspan 4 -columnspan 2 -pady 3m
    ttk::button .lbl1.btnInformation -text "[lang TkAddonInformation]..." -width 14
    grid .lbl1.btnInformation -column 3 -row 0 -sticky nw -pady 1m -padx 1m 
    ttk::button .lbl1.btnUnnistall -text [lang TkAddonUnnistall] -width 14
    grid .lbl1.btnUnnistall -column 3 -row 1 -sticky nw -pady 1m -padx 1m 
    ttk::button .lbl1.btnConfigure -text [lang TkAddonConfigure] -width 14
    grid .lbl1.btnConfigure -column 3 -row 2 -sticky nw -pady 1m -padx 1m 
    ttk::button .lbl1.btnHelp -text "[lang TkAddonHelp]..." -width 14
    grid .lbl1.btnHelp -column 3 -row 3 -sticky nw -pady 1m -padx 1m
    
    ttk::labelframe .lbl2 -text [lang TkMoreAddon] -width 280 -height 180
    listbox .lbl2.list -width 30
    pack .lbl2 -fill x -padx 5
    grid .lbl2.list -column 0 -rowspan 4 -columnspan 2 -pady 3m
    ttk::button .lbl2.btnInformation -text "[lang TkAddonInformation]..." -width 14
    grid .lbl2.btnInformation -column 3 -row 0 -sticky nw -pady 1m -padx 1m
    ttk::button .lbl2.btnUnnistall -text [lang TkAddonInstall] -width 14
    grid .lbl2.btnUnnistall -column 3 -row 1 -sticky nw -pady 1m -padx 1m
    ttk::button .lbl2.btnConfigure -text "[lang TkAddonDelete]..." -width 14
    grid .lbl2.btnConfigure -column 3 -row 2 -sticky nw -pady 1m -padx 1m
    
    ttk::button .btnClose -text [lang TkClose] -command [list wm withdraw .]
    pack .btnClose -padx 5 -anchor e
    
    set addons [glob $pnp(addonDir)/*.tcl]
    foreach addon $addons {
        set n [file rootname [file tail $addon]]
        if {[lsearch $::cfg(addons) $n] == -1} {
            .lbl2.list insert end "$n   \($addon\)"
        } else {
            .lbl1.list insert end "$n   \($addon\)"
        }
    }

}

proc tkCleanUp {} {
    foreach id [winfo children .] {
        destroy $id
    }
}
wm protocol . WM_DELETE_WINDOW chk_exit

proc chk_exit {} {
    wm witdhraw .
}

wm deiconify .