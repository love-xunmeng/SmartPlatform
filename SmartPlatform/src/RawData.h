#ifndef raw_data_h_
#define raw_data_h_

#include <memory>

class RawData {
public:
	RawData(const char *data, const int data_size) {
		data_ = nullptr;
		data_size_ = 0;
		if (data_size > 0)
		{
			data_size_ = data_size;
			data_ = new char[data_size_];
			memcpy(data_, data, data_size_);
		}
	}

	~RawData()
	{
		if (data_)
		{
			delete[]data_;
			data_ = nullptr;
			data_size_ = 0;
		}
	}

	char* data() {
		return data_;
	}

	int data_size() {
		return data_size_;
	}

private:
	RawData() = delete;
	RawData(const RawData &obj) = delete;
	RawData& operator=(const RawData& obj) = delete;

private:
	char* data_;
	int data_size_;
};

#endif //raw_data_h_
