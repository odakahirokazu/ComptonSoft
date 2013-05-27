class ComptonEventFilter : public anl::BasicModule
{
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  void define_condition();
  void add_condition(const std::string& type,
                     double min_value, double max_value);
};
