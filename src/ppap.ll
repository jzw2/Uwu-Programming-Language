; ModuleID = 'module'

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00"

define i64 @fibonacci(i64 %nth) {
"entry point":
  %0 = alloca i64
  store i64 %nth, i64* %0
  %nth1 = load i64, i64* %0
  %camping = icmp eq i64 %nth1, 0
  br i1 %camping, label %then, label %else

then:                                             ; preds = %"entry point"
  ret i64 0

else:                                             ; preds = %"entry point"
  %nth2 = load i64, i64* %0
  %camping3 = icmp eq i64 %nth2, 1
  br i1 %camping3, label %then4, label %else5

then4:                                            ; preds = %else
  ret i64 1

else5:                                            ; preds = %else
  br label %merge

merge:                                            ; preds = %else5
  br label %merge6

merge6:                                           ; preds = %merge
  %nth7 = load i64, i64* %0
  %subtracting = sub i64 %nth7, 1
  %"calling the function" = call i64 @fibonacci(i64 %subtracting)
  %nth8 = load i64, i64* %0
  %subtracting9 = sub i64 %nth8, 2
  %"calling the function10" = call i64 @fibonacci(i64 %subtracting9)
  %adding = add i64 %"calling the function", %"calling the function10"
  %1 = alloca i64
  store i64 %adding, i64* %1
  %temp = load i64, i64* %1
  %"calling the function11" = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i64 %temp)
  %temp12 = load i64, i64* %1
  ret i64 %temp12
}

declare i32 @printf(i8*, ...)

define i64 @main() {
"entry point":
  %"calling the function" = call i64 @fibonacci(i64 20)
  %"calling the function1" = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i64 %"calling the function")
  ret i64 0
}
