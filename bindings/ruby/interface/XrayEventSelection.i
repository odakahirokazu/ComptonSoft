class XrayEventSelection : public anlnext::BasicModule
{
public:
  XrayEventSelection();
  ~XrayEventSelection();

  void add_condition(const std::string& key, std::vector<std::pair<int, int>>& range);
  void add_condition(const std::string& key, std::vector<std::pair<double, double>>& range);
};
