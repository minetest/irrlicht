
set(IRREXAMPLES
	01.HelloWorld
	02.Quake3Map
	03.CustomSceneNode
	04.Movement
	05.UserInterface
	06.2DGraphics
	07.Collision
	08.SpecialFX
	09.Meshviewer
	10.Shaders
	11.PerPixelLighting
	12.TerrainRendering
	13.RenderToTexture
	16.Quake3MapShader
	18.SplitScreen
	19.MouseAndJoystick
	20.ManagedLights
	21.Quake3Explorer
	22.MaterialViewer
	23.SMeshHandling
	24.CursorControl
	26.OcclusionQuery
	27.PostProcessing
	28.CubeMapping
	30.Profiling
)

if(WIN32)
	list(APPEND IRREXAMPLES 14.Win32Window)
endif()
if(UNIX)
	list(APPEND IRREXAMPLES AutomatedTest)
endif()

foreach(exname IN ITEMS ${IRREXAMPLES})
	file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/${exname}/*.cpp")
	add_executable(${exname} ${sources})

	target_include_directories(${exname} PRIVATE
		${CMAKE_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/${exname}
	)
	target_link_libraries(${exname} IrrlichtMt)
endforeach()
