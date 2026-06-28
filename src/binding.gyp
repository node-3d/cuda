{
	'variables': {
		'bin'        : '<!(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printBin())")',
		'gl_include' : '<!(node -p "require(\'@node-3d/deps-opengl\').include")',
		'gl_bin'     : '<!(node -p "require(\'@node-3d/deps-opengl\').bin")',
	},
	'conditions': [[
		'OS=="win"',
		{'variables': {'obj': 'obj'}},
		{'variables': {'obj': 'o'}},
	]],
	"targets": [
		{
			"target_name": "cuda",
			"includes": ["common.gypi"],
			"sources": [
				"cpp/bindings.cpp",
				"cpp/ctx.cpp",
				"cpp/device.cpp",
				"cpp/function.cpp",
				"cpp/mem.cpp",
				"cpp/modulex.cpp",
				"cpp/thrust_func.cu",
			],
			'include_dirs': [
				'<(gl_include)',
				'<!@(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printInclude())")',
			],
			'library_dirs': ['<(gl_bin)'],
			'rules': [{
				'extension': 'cu',
				'inputs': ['<(RULE_INPUT_PATH)'],
				'outputs':[ '<(INTERMEDIATE_DIR)/<(RULE_INPUT_ROOT).<(obj)'],
				'message': "Compiling CUDA files...",
				'conditions': [
					['OS=="win"', {
						'rule_name': 'cuda-windows',
						'process_outputs_as_sources': 0,
						'action': ['node', 'build-cuda.js', '<(_inputs)', '<(_outputs)'],
					}],
					['OS!="win"', {
						'rule_name': 'cuda-unix',
						'process_outputs_as_sources': 1,
						'action': ['node', 'build-cuda.js', '<(_inputs)', '<(_outputs)'],
					}],
				]
			}],
			'conditions': [
				[ 'OS=="mac"', {
					'libraries': ['-framework CUDA'],
					'include_dirs': ['/usr/local/include', '/usr/local/cuda/include'],
					'library_dirs': ['/usr/local/lib']
				}],
				[ 'OS=="linux"', {
					'libraries': ['-lcuda', '-lcudart', '-lnvrtc'],
					'include_dirs': ['/usr/local/include', '/usr/local/cuda/include'],
					'library_dirs': ['/usr/local/lib']
				}],
				[ 'OS=="win"', {
					'defines' : [
						'WIN32_LEAN_AND_MEAN',
						'VC_EXTRALEAN',
						'_WIN32',
					],
					'libraries': [
						'-l$(CUDA_PATH)/lib/x64/nvrtc.lib',
						'-l$(CUDA_PATH)/lib/x64/cuda.lib',
						'-l$(CUDA_PATH)/lib/x64/cudart.lib',
						'glew32.lib',
						'opengl32.lib',
					],
					"include_dirs": [
						"$(CUDA_PATH)/include",
					],
				}]
			]
		}
	]
}
