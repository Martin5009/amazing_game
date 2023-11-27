if {[catch {

# define run engine funtion
source [file join {C:/lscc/radiant/3.2} scripts tcl flow run_engine.tcl]
# define global variables
global para
set para(gui_mode) 1
set para(prj_dir) "C:/Users/msusanto/Desktop/my_designs/matrix_driver_ms"
# synthesize IPs
# synthesize VMs
# synthesize top design
file delete -force -- matrix_driver_ms_impl_1.vm matrix_driver_ms_impl_1.ldc
run_engine_newmsg synthesis -f "matrix_driver_ms_impl_1_lattice.synproj"
run_postsyn [list -a iCE40UP -p iCE40UP5K -t SG48 -sp High-Performance_1.2V -oc Industrial -top -w -o matrix_driver_ms_impl_1_syn.udb matrix_driver_ms_impl_1.vm] "C:/Users/msusanto/Desktop/my_designs/matrix_driver_ms/impl_1/matrix_driver_ms_impl_1.ldc"

} out]} {
   runtime_log $out
   exit 1
}
