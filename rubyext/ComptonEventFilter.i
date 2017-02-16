class ComptonEventFilter : public anl::BasicModule
{
public:
  ComptonEventFilter();
  ~ComptonEventFilter() {}

  void define_condition();
  void add_hit_pattern(const std::string& name);
  void add_evs_key(const std::string& key);
  void add_condition(const std::string& type,
                     double min_value, double max_value);
};
