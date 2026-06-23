%nodefault;
class VANLGeometry : public anlnext::BasicModule
{
public:
  double get_length_unit() const;
  std::string get_length_unit_name() const;
};
%makedefault;
