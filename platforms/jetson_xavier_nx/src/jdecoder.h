#ifndef JDECODER_H_
#define JDECODER_H_

#include <vector>
#include <memory>
#include <cstdlib>

#define EMBD_DIM    128

namespace mot {

struct Detection {
    int32_t category;
    float score;
    struct {
        float top;
        float left;
        float bottom;
        float right;
    } bbox;
    float embedding[EMBD_DIM];
};

class JDecoder
{
public:
    static JDecoder* instance(void);
    bool init(void);
    bool infer(std::vector<std::shared_ptr<float>>& in, std::vector<Detection>& dets);
    bool destroy(void);
private:
    static JDecoder* me;
    enum
    {
        NUM_OUTPUTS = 3,
        NUM_ANCHORS = 12
    };
    const int32_t num_classes;
    const int32_t num_boxes;
    float conf_thresh;
    float iou_thresh;
    const float biases[NUM_ANCHORS * 2];
    const int32_t masks[NUM_ANCHORS];
    const int32_t inwidth;
    const int32_t inheight;
    const int32_t strides[NUM_OUTPUTS];
    JDecoder();
    ~JDecoder() {};
};

}   // namespace mot

#endif