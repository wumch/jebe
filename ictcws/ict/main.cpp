
#include <iostream>
#include "utility.hpp"
#include "result.hpp"
#include "../staging/mbswcs.hpp"
#include <string.h>

int main(int argc, char* argv[]) {
	CResult worker;
	int		outputFormat = 1;
	int		operateType = 1;
	char *content = argv[1];
	char in[10000];
	staging::mbswcs::code_convert("utf-8", "gbk", content, strlen(content), in, 10000);

	char result[10000];
	worker.m_nOperateType = operateType;
	worker.m_nOutputFormat = outputFormat;
	worker.ParagraphProcessing(in, result);

	char out[10000];
	staging::mbswcs::code_convert("gbk", "utf-8", result, strlen(result), out, 10000);

    std::cout<<out<<std::endl;
	return 0;
}
