# Searching-Software
Công cụ Tìm kiếm Mã nguồn C++ cho Windows
Đây là một công cụ dòng lệnh (command-line) mạnh mẽ được viết bằng C++17, được thiết kế để tìm kiếm văn bản và từ khóa trong các tệp mã nguồn C/C++ (.c, .cpp, .h) một cách hiệu quả ngay trên môi trường Windows.

Tính năng Nổi bật
Đăng nhập Cơ bản: Một màn hình đăng nhập đơn giản để hạn chế quyền truy cập.
//TK: longurara
//MK: test1234

Tìm kiếm Đệ quy: Tự động quét tất cả các thư mục con từ một thư mục gốc được chỉ định.

Hỗ trợ Từ khóa Nâng cao:

Tìm kiếm nhiều từ khóa cùng lúc (phân tách bằng dấu phẩy).

Tự động tạo biến thể từ khóa tiếng Việt: Chương trình thông minh chuyển đổi các từ khóa có dấu thành không dấu và các biến thể có gạch dưới (ví dụ: tìm kiếm sẽ khớp với tim kiem, timkiem, và tim_kiem).

Tùy chọn Tìm kiếm Linh hoạt:

Tìm kiếm không phân biệt chữ hoa/thường.

Tìm kiếm khớp toàn bộ từ (whole word matching).

Hiển thị Kết quả Trực quan:

Giao diện kết quả được tô màu để dễ đọc.

Kết quả được nhóm theo tệp và hiển thị rõ ràng số dòng và nội dung dòng đó.

Từ khóa tìm thấy được tô sáng trực tiếp trên dòng kết quả.

Tương tác sau Tìm kiếm: Cho phép người dùng chọn và mở một tệp kết quả trực tiếp bằng trình soạn thảo mặc định của hệ thống.

Hỗ trợ Unicode (UTF-8): Xử lý tốt các đường dẫn tệp và nội dung có chứa ký tự Unicode, đảm bảo hoạt động chính xác với tiếng Việt.

Yêu cầu Hệ thống
Hệ điều hành: Windows (do sử dụng trực tiếp Windows API như windows.h, conio.h, ShellExecuteW).

Trình biên dịch: Yêu cầu hỗ trợ chuẩn C++17 hoặc mới hơn (ví dụ: g++, MSVC).

Hướng dẫn Biên dịch
Chương trình này yêu cầu chuẩn C++17. Vui lòng sử dụng cờ -std=c++17 khi biên dịch với g++.

Ví dụ với g++:

Bash

g++ -std=c++17 -o TimKiem.exe timkiem.cpp
Lưu ý: Không cần sử dụng cờ -lstdc++fs với phiên bản mã nguồn này.

Hướng dẫn Sử dụng
Chạy chương trình: Mở một terminal (như Command Prompt hoặc PowerShell) và chạy tệp TimKiem.exe.

Đăng nhập: Chương trình sẽ yêu cầu thông tin đăng nhập. Sử dụng thông tin sau:

Tên đăng nhập: longurara

Mật khẩu: test1234
(Mật khẩu sẽ được che bằng dấu * khi bạn nhập).

Nhập đường dẫn: Sau khi đăng nhập thành công, nhập đường dẫn tuyệt đối đến thư mục chứa mã nguồn bạn muốn tìm kiếm.

Công cụ tìm kiếm mã nguồn C++ (Bản ổn định cho Windows)
Vui lòng nhập đường dẫn đến thư mục cần tìm:
> D:\projects\my_cpp_project
Bắt đầu Tìm kiếm:

Nhập các từ khóa bạn muốn tìm, cách nhau bằng dấu phẩy. Nhập q để thoát.

Chọn y hoặc n cho tùy chọn "không phân biệt chữ hoa/thường".

Chọn y hoặc n cho tùy chọn "chỉ tìm kiếm toàn bộ từ".

Xem kết quả: Chương trình sẽ quét các tệp và hiển thị kết quả tìm thấy.

1: D:\projects\my_cpp_project\src\main.cpp
  L25: // TODO: Cần phải tối ưu hóa hàm tìm kiếm này.

2: D:\projects\my_cpp_project\include\utils.h
  L12: std::string perform_search(const std::string& query);
Mở tệp: Sau khi hiển thị kết quả, bạn có thể nhập số thứ tự của tệp (ví dụ: 1, 2,...) để mở tệp đó bằng ứng dụng mặc định. Nhập 0 để quay lại và thực hiện một tìm kiếm mới.

Giải thích Chức năng Đặc biệt
Xử lý Từ khóa Tiếng Việt
Đây là một trong những tính năng mạnh mẽ nhất của công cụ này. Khi bạn nhập một từ khóa có dấu, ví dụ "thực thể", chương trình sẽ tự động tìm kiếm các biến thể sau:

thực thể (nguyên bản)

thucthe (không dấu, viết liền)

thuc_the (không dấu, dùng gạch dưới)

Điều này cực kỳ hữu ích khi tìm kiếm trong các bình luận hoặc chuỗi văn bản trong mã nguồn mà có thể đã được viết không nhất quán (có dấu hoặc không dấu).

Phụ thuộc
Thư viện chuẩn C++17.

Windows API:

windows.h: Dùng cho các chức năng hệ thống cơ bản, xử lý console và duyệt tệp.

conio.h: Dùng cho _getch() để che mật khẩu khi nhập.
