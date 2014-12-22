## this library was used to define some utilize to help building parts_db
if {[info exists otto_lib]==0} {
    variable otto_lib 1
    proc namespace_dump {ns} {
    set ll [lsort -nocase [info vars ::${ns}::*]]
    foreach arg $ll {
        set nn [namespace tail $arg]
        set v [set $arg]
        puts "${nn}=$v \(${arg}\)"
    }
    }
    proc namespace_copy {dst src} {
        namespace eval ::${dst} {}
        set ll [info vars ::${src}::*]
        foreach arg $ll {
            set nn [namespace tail $arg]
            variable ::${dst}::${nn} [set ::${src}::${nn}]
        }
    }
}

