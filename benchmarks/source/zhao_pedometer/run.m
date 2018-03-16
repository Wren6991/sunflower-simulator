--	Command:	SIGSRC
--
--	Description:	Create a physical phenomenon signal source.
--
--	Arguments:	<type> <description> <tau> <propagationspeed> <A\
--			> <B> <C> <D> <E> <F> <G> <H> <I> <K> <m> <n\
--			> <o> <p> <q> <r> <s> <t> <x> <y> <z> <traje\
--			ctoryfile> <trajectoryrate> <looptrajectory>\
--			 <samplesfile> <samplerate> <fixedsampleval>\
--			 <loopsamples>	

-- our signals have a simple non-attenuative model (modelling a noise-free ADC)

sigsrc 0 "Accel X" 0.0 0.0   1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 " " 100 0.0 0.0 0.0 1 "accelx.svt" 0 1000.0 0
sigsrc 0 "Accel Y" 0.0 0.0   1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 " " 100 0.0 0.0 0.0 1 "accely.svt" 0 1000.0 0
sigsrc 0 "Accel Z" 0.0 0.0   1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 " " 100 0.0 0.0 0.0 1 "accelz.svt" 0 1000.0 0
sigsrc 0 "Activity" 0.0 0.0   1.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0    0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 " " 100 0.0 0.0 0.0 1 "activity.svt" 0 1000.0 0

sizemem 96000000
sigsubscribe 0 0
sigsubscribe 1 1
sigsubscribe 2 2
sigsubscribe 3 3
srecl "pedometer.sr"
run
on
