#include <charconv>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include <png++/png.hpp>

class PngInput {
 public:
  PngInput(const std::string& path, int x_shift=0, int y_shift=0) :
    path_{path}, x_shift_{x_shift}, y_shift_{y_shift} {
    image_.read(path);
  }
  const png::rgb_pixel& GetPixel(int x, int y) const { return image_[x][y]; }
 private:
  const std::string path_;
  const int x_shift_;
  const int y_shift_;
  png::image<png::rgb_pixel> image_;
};

static int GetImages(
    std::vector<PngInput> &pngs,
    const std::vector<std::string>& paths_xys) {
  int rc = 0;
  pngs.reserve(paths_xys.size());
  for (const std::string& path_xy: paths_xys) {
    std::string path{path_xy};
    const char *cs = path_xy.data(), *cse = cs + path_xy.size();
    int x_shift = 0, y_shift = 0;
    auto w = path_xy.find('+');
    if (w != std::string::npos) {
      path = std::string(cs, w);
      auto fc_rc = std::from_chars(cs + w + 1, cse, x_shift);
      if (fc_rc.ec == std::errc()) {
        fc_rc = std::from_chars(fc_rc.ptr + 1, cse, y_shift);
      }
      if ((rc == 0) && (fc_rc.ec != std::errc())) {
        rc = 1;
        std::cerr << "Bad input option in " << path_xy << '\n';
      }
    }
    pngs.push_back(PngInput(path, x_shift, y_shift));
  }
  return rc;
}

int main(int argc, char **argv) {
  namespace po = boost::program_options;
  int rc = 0;
  po::variables_map vm;
  po::options_description desc("3333-server options");
  desc.add_options()
    ("help", "produce help message")
    ("input,i",
       po::value<std::vector<std::string>>()->multitoken()->required(),
       "input PNG path with optional +<x>+<y>")
    ("output,o", po::value<std::string>()->required(), "output PNG path")
    ("debug", po::value<std::string>()->default_value("0"), "Debug flags")
  ;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << "\n";
  } else {
    std::vector<PngInput> in_pngs;
    GetImages(in_pngs, vm["input"].as<std::vector<std::string>>());
  }
  return rc;
}
