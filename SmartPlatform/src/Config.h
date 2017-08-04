#ifndef config_h_
#define config_h_

#include <map>
#include <string>

class Config {
public:
	struct CigaretteNode {
		int train_id_;
		int db_id_;
		std::string name_;
	};

	Config();
	~Config();
	static Config* instance();
	int init();
	int get_db_id_by_train_id(int train_id);
	bool is_save_image();


	inline std::string image_save_root_dir() const {
		return image_save_root_dir_;
	}

private:
	int load_cigarette_map();
	void debug_print_all();

private:
	static Config* instance_;
	std::map<int, CigaretteNode> cigarette_map_;
	bool is_save_image_;
	std::string image_save_root_dir_;
};

#endif // !config_h_

