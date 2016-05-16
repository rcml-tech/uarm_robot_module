class UarmError{
  ::std::string error_text;
  bool is_empty;
  ::std::vector<UarmError*> content;
public:
  UarmError() : error_text(""), is_empty(true){};
  explicit UarmError(::std::string text) : error_text(text), is_empty(false){};
  void append(UarmError *e);
  ::std::string emit();
  bool isEmpty();
  ~UarmError();
};