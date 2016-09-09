import FWCore.ParameterSet.Config as cms

#-----------------
#	HCAL DQM Offline Source Sequence Definition for Heavy Ions
#	To be used for Offline DQM importing
#-----------------

#	import the tasks
from DQM.HcalTasks.DigiTask import digiTask
from DQM.HcalTasks.DigiPhase1Task import digiPhase1Task
from DQM.HcalTasks.RawTask import rawTask
from DQM.HcalTasks.TPTask import tpTask
from DQM.HcalTasks.RecHitTask import recHitTask

#	set processing type to Offine
digiTask.ptype = cms.untracked.int32(1)
from DQM.HcalTasks.DigiPhase1Task import digiPhase1Task
tpTask.ptype = cms.untracked.int32(1)
recHitTask.ptype = cms.untracked.int32(1)
rawTask.ptype = cms.untracked.int32(1)

#	set the run key(value and name)
digiTask.runkeyVal = cms.untracked.int32(4)
tpTask.runkeyVal = cms.untracked.int32(4)
recHitTask.runkeyVal = cms.untracked.int32(4)
rawTask.runkeyVal = cms.untracked.int32(4)

digiTask.runkeyName = cms.untracked.string("hi_run")
tpTask.runkeyName = cms.untracked.string("hi_run")
recHitTask.runkeyName = cms.untracked.string("hi_run")
rawTask.runkeyName = cms.untracked.string("hi_run")

#	Set the Emulator label for TP Task
tpTask.tagEmul = cms.untracked.InputTag("valHcalTriggerPrimitiveDigis")

hcalOfflineSourceSequence = cms.Sequence(
	digiTask
	+tpTask
	+recHitTask
	+rawTask)

_phase1_hcalOfflineSourceSequence = hcalOfflineSourceSequence.copy() 
_phase1_hcalOfflineSourceSequence.insert(_phase1_hcalOfflineSourceSequence.index(digiTask),digiPhase1Task)

from Configuration.StandardSequences.Eras import eras
eras.run2_HCAL_2017.toReplaceWith( hcalOfflineSourceSequence, _phase1_hcalOfflineSourceSequence )

