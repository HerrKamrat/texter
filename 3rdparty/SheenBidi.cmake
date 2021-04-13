cmake_minimum_required(VERSION 2.8)

project(sheenbidi)

set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/SheenBidi/Source")

add_library(${PROJECT_NAME}
${SRC_DIR}/BidiChain.c
${SRC_DIR}/BidiChain.h
${SRC_DIR}/BidiTypeLookup.c
${SRC_DIR}/BidiTypeLookup.h
${SRC_DIR}/BracketQueue.c
${SRC_DIR}/BracketQueue.h
${SRC_DIR}/BracketType.h
${SRC_DIR}/GeneralCategoryLookup.c
${SRC_DIR}/GeneralCategoryLookup.h
${SRC_DIR}/IsolatingRun.c
${SRC_DIR}/IsolatingRun.h
${SRC_DIR}/LevelRun.c
${SRC_DIR}/LevelRun.h
${SRC_DIR}/PairingLookup.c
${SRC_DIR}/PairingLookup.h
${SRC_DIR}/RunExtrema.h
${SRC_DIR}/RunKind.h
${SRC_DIR}/RunQueue.c
${SRC_DIR}/RunQueue.h
${SRC_DIR}/SBAlgorithm.c
${SRC_DIR}/SBAlgorithm.h
${SRC_DIR}/SBAssert.h
${SRC_DIR}/SBBase.c
${SRC_DIR}/SBBase.h
${SRC_DIR}/SBCodepointSequence.c
${SRC_DIR}/SBCodepointSequence.h
${SRC_DIR}/SBLine.c
${SRC_DIR}/SBLine.h
${SRC_DIR}/SBLog.c
${SRC_DIR}/SBLog.h
${SRC_DIR}/SBMirrorLocator.c
${SRC_DIR}/SBMirrorLocator.h
${SRC_DIR}/SBParagraph.c
${SRC_DIR}/SBParagraph.h
${SRC_DIR}/SBScriptLocator.c
${SRC_DIR}/SBScriptLocator.h
${SRC_DIR}/ScriptLookup.c
${SRC_DIR}/ScriptLookup.h
${SRC_DIR}/ScriptStack.c
${SRC_DIR}/ScriptStack.h
${SRC_DIR}/SheenBidi.c
${SRC_DIR}/StatusStack.c
${SRC_DIR}/StatusStack.h
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/SheenBidi/Headers")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/SheenBidi/Source")

#target_compile_definitions(${PROJECT_NAME} PUBLIC SB_CONFIG_UNITY)
