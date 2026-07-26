{
	'variables': {
		'bin'        : '<!(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printBin())")',
		'gl_include' : '<!(node -p "require(\'@node-3d/deps-opengl\').include")',
		'gl_bin'     : '<!(node -p "require(\'@node-3d/deps-opengl\').bin")',
		'cuda_path'  : '<!(node cuda-path.js)',
	},
	'conditions': [[
		'OS=="win"',
		{'variables': {'obj': 'obj'}},
		{'variables': {'obj': 'o'}},
	], [
		'OS=="win" and target_arch=="arm64"',
		{'variables': {'cuda_lib_arch': 'arm64'}},
		{'variables': {'cuda_lib_arch': 'x64'}},
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
						'action': ['node', 'build-cuda.js', '<(_inputs)', '<(_outputs)', '<(target_arch)'],
					}],
					['OS!="win"', {
						'rule_name': 'cuda-unix',
						'process_outputs_as_sources': 1,
						'action': ['node', 'build-cuda.js', '<(_inputs)', '<(_outputs)', '<(target_arch)'],
					}],
				]
			}],
			'conditions': [
				[ 'OS=="mac"', {
					'libraries': ['-framework CUDA'],
					'include_dirs': ['/usr/local/include', '<(cuda_path)/include', '/usr/local/cuda/include'],
					'library_dirs': ['/usr/local/lib']
				}],
				[ 'OS=="linux"', {
					'libraries': ['-lcuda', '-lcudart', '-lnvrtc'],
					'include_dirs': ['/usr/local/include', '<(cuda_path)/include', '/usr/local/cuda/include'],
					'library_dirs': [
						'/usr/local/lib',
						'<(cuda_path)/lib64',
						'<(cuda_path)/lib64/stubs',
						'/usr/local/cuda/lib64',
						'/usr/local/cuda/lib64/stubs'
					]
				}],
				[ 'OS=="win"', {
					'defines' : [
						'WIN32_LEAN_AND_MEAN',
						'VC_EXTRALEAN',
						'_WIN32',
					],
					'libraries': [
						'-l<(cuda_path)/lib/<(cuda_lib_arch)/nvrtc.lib',
						'-l<(cuda_path)/lib/<(cuda_lib_arch)/cuda.lib',
						'-l<(cuda_path)/lib/<(cuda_lib_arch)/cudart.lib',
						'glew32.lib',
						'opengl32.lib',
					],
					"include_dirs": [
						"<(cuda_path)/include",
					],
				}]
			]
		}
	]
}
