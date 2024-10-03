#include <array>
#include <charconv>
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
#include <png++/png.hpp>

using ai2_t = std::array<int, 2>;

class PngInput {
 public:
  PngInput(const std::string& path, int x_shift=0, int y_shift=0) :
    path_{path}, x_shift_{x_shift}, y_shift_{y_shift} {
    image_.read(path);
    std::cerr << path << " w="<<Width() << " h="<<Height() << '\n';
  }
  const png::rgb_pixel& GetPixel(int x, int y) const { return image_[y][x]; }
  const png::rgb_pixel GetPixelShited(int x, int y) const {
    return GetPixel(x - x_shift_, y - y_shift_);
  }
  size_t Width() const { return image_.get_width(); }
  size_t Height() const { return image_.get_height(); }
  ai2_t Shift() const { return ai2_t{x_shift_, y_shift_}; }
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

static void MaxBy(int &v, int x) {
  if (v < x) { v = x; }
}

template <typename T>
static void MinBy(T &v, T x) {
  if (v > x) { v = x; }
}

static ai2_t GetOutputSize(const std::vector<PngInput>& inputs) {
  int w_out = 0, h_out = 0;
  for (const PngInput &i: inputs) {
    const ai2_t shift = i.Shift();
    MaxBy(w_out, i.Width() + shift[0]);
    MaxBy(h_out, i.Height() + shift[1]);
  }
  std::cerr << "w_out="<<w_out << " h_out="<<h_out << '\n';
  return ai2_t{w_out, h_out};
}

static int Combine(
    png::image<png::rgb_pixel> &out,
    const std::vector<PngInput>& inputs) {
  int rc = 0;
  const int w_out = out.get_width();
  const int h_out = out.get_height();
  for (int x = 0; x < w_out; ++x) {
    for (int y = 0; y < h_out; ++y) {
      png::rgb_pixel& pixel = out[y][x];
      pixel.red = pixel.green = pixel.blue = 0xff;
      for (const PngInput &i: inputs) {
        const ai2_t shift = i.Shift();
        if ((shift[0] <= x) && (x < i.Width() + shift[0]) &&
            (shift[1] <= y) && (y < i.Height() + shift[1])) {
          png::rgb_pixel ipixel = i.GetPixelShited(x, y);
          MinBy(pixel.red, ipixel.red);
          MinBy(pixel.green, ipixel.green);
          MinBy(pixel.blue, ipixel.blue);
        }
      }
    }
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
    ai2_t out_size = GetOutputSize(in_pngs);
    png::image<png::rgb_pixel> png_out(out_size[0], out_size[1]);
    rc = Combine(png_out, in_pngs);
    if (rc == 0) {
      png_out.write(vm["output"].as<std::string>());
    }
  }
  return rc;
}
