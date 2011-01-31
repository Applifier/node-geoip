import os

srcdir = os.path.abspath('.')
blddir = 'build'

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.env.append_unique('LINKFLAGS',["-L/opt/local/lib/"]);
  conf.env.append_unique('CXXFLAGS',["-I/opt/local/include/"]);
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cxx(lib='GeoIP', mandatory=True, uselib_store='GeoIP')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.uselib = 'GeoIP'
  obj.target = "geoip"
  obj.source = "geoip.cc"

  filename = '%s.node' % obj.target
  from_path = os.path.join(srcdir, blddir, 'default', filename)
  to_path   = os.path.join(srcdir, filename)

  if os.path.exists(from_path):
     os.rename(from_path, to_path)
