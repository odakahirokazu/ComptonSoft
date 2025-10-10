#include "VLArRecombinationModelFactory.hh"
#include "BirksModel.hh"
#include "CSException.hh"
#include "ModifiedBoxModel.hh"
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <map>
namespace comptonsoft {
std::unique_ptr<VLArRecombinationModel> VLArRecombinationModelFactory::load(std::string const &filename) {
  boost::property_tree::ptree pt;
  try {
    read_xml(filename, pt);
  }
  catch (boost::property_tree::xml_parser_error &ex) {
    std::ostringstream message;
    message << "Cannot parse: " << filename << "\n"
            << "xml_parser_error ===> \n"
            << "message: " << ex.message() << "\n"
            << "filename: " << ex.filename() << "\n"
            << "line: " << ex.line() << "\n";
    BOOST_THROW_EXCEPTION(CSException(message.str()));
  }
  auto node = pt.find("RecombinationModel");
  const auto model = node->second.get_optional<std::string>("<xmlattr>.model");
  if (model == boost::none) {
    std::ostringstream message;
    message << "Error: Recombination model is not assigned in " << filename << "\n";
    BOOST_THROW_EXCEPTION(CSException(message.str()));
  }
  RecombinationModelType type;
  const auto parameter_node = node->second.get_child_optional("parameters");
  std::map<std::string, double> params;
  if (parameter_node != boost::none) {
    bool found = false;
    for (const auto &m: *parameter_node) {
      if (m.first != "model") continue;
      const auto type_attr = m.second.get_optional<std::string>("<xmlattr>.type");
      if (!type_attr) continue;
      if (*type_attr != *model) continue;
      found = true;
      for (const auto &p: m.second) {
        if (p.first != "parameter") continue;
        const auto name_attr = p.second.get_optional<std::string>("<xmlattr>.name");
        const auto value_attr = p.second.get_optional<std::string>("<xmlattr>.value");
        if (!name_attr || !value_attr) {
          std::ostringstream message;
          message << "Error: parameter element missing name or value attribute in " << filename << "\n";
          BOOST_THROW_EXCEPTION(CSException(message.str()));
        }
        try {
          params.insert(std::make_pair(*name_attr, boost::lexical_cast<double>(*value_attr)));
        }
        catch (boost::bad_lexical_cast &ex) {
          std::ostringstream message;
          message << "Error: Invalid parameter value for " << *name_attr << " in " << filename << "\n"
                  << "bad_lexical_cast ===> \n"
                  << ex.what() << "\n";
          BOOST_THROW_EXCEPTION(CSException(message.str()));
        }
      }
      break; // stop after the matching model
    }
    if (!found) {
      std::cout << "Info: Parameters for " << *model << " model is not found in " << filename << ". Use default parameters." << std::endl;
    }
  }
  if (*model == "Birks") {
    type = RecombinationModelType::Birks;
  }
  else if (*model == "ModifiedBox") {
    type = RecombinationModelType::ModifiedBox;
  }
  else if (*model == "None") {
    type = RecombinationModelType::None;
  }
  else {
    std::ostringstream message;
    message << "Error: Unknown recombination model: " << *model << " in " << filename << "\n";
    BOOST_THROW_EXCEPTION(CSException(message.str()));
  }
  return create(type, params);
}
std::unique_ptr<VLArRecombinationModel> VLArRecombinationModelFactory::create(RecombinationModelType type, const std::map<std::string, double> &params) {
  switch (type) {
  case RecombinationModelType::Birks:
    return std::make_unique<BirksModel>(params);
  case RecombinationModelType::ModifiedBox:
    return std::make_unique<ModifiedBoxModel>(params);
  case RecombinationModelType::None:
    return nullptr;
  default: {
    std::ostringstream message;
    message << "Error: Unknown recombination model type\n";
    BOOST_THROW_EXCEPTION(CSException(message.str()));
  }
  }
}
} // namespace comptonsoft