require 'comptonsoft/database'

# d = ComptonSoft::Database::DetectorConfiguration.new
# d.read "detector_configuration.xml"
# d = ComptonSoft::Database::ChannelMap.new
# d.read "channel_map.xml"
# d = ComptonSoft::Database::ChannelPropertiesDB.new
# d.read "channel_properties.xml"
d = ComptonSoft::Database::DetectorParametersDB.new
d.read "detector_parameters.xml"
p d
