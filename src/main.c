int main(int argc, char*argv[])
{
	char input[] = "2345";
	char output[] = "1234";
	char bias[] = "9";
	char *args[] = {"result.c","../testcase/a.input","../testcase/a.output","9"};
	gen(4,args);

	login(argv[1],argv[2],argv[3]);
	login(argv[1],argv[2],argv[3]);

	diff("diff","diff",0);

	return 0;
}
