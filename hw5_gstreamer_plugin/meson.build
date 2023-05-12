project('gstreamer-plugin', 'c', version : '1.0.0', license : 'LGPL')

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

cc = meson.get_compiler('c')
gst_version = meson.project_version()
api_version = '1.0'
gst_dep = dependency('gstreamer-1.0', version : '>=1.0',
    required : true, fallback : ['gstreamer', 'gst_dep'])

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'wavlesrc')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'WAV LE16 source')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/AlexanderSokolkin/otus-c-2023-01')


plugin_sources = [
  'gstwavlesrc.c'
]

gstpluginwav = library('gstwavlesrc',
  plugin_sources,
  dependencies : [gst_dep],
  install : true,
  install_dir : plugins_install_dir,
)
