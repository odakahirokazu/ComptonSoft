%nodefault;
class VANLGeometry : public anl::BasicModule
{
public:
  void SetLengthUnit(const std::string& name);
  double GetLengthUnit() const;
  std::string GetLengthUnitName() const;
};
%makedefault;
