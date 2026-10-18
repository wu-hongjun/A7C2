# 原生模組與可重現檢查

`assets/manifest.json` 固定每個實際部署資產的SHA-256。`native/` 包含原生AArch64程式與固定窗口表；`installer.c` 的本機資料指標需要80個 ELF RELATIVE 重定位，由機內正常載入器處理。沒有外部符號或 DT_NEEDED 相依庫。原生診斷、WBI及執行期讀取模組沒有動態相依或重定位。

參考交叉編譯器為 Zig 0.13.0：

```text
zig cc -target aarch64-linux-gnu -shared -fPIC -O2 -ffreestanding -fno-builtin -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlib -Wl,--no-undefined -Wl,-Bsymbolic -Wl,-z,noexecstack -o installer.so native/installer.c
```

以相同參數分別編譯 `probe.c`、`wbi.c`、`runtime.c`。`runtime.c` 包含 `probe.c`。工具鏈差異可能造成輸出位元組不同；檔案不同不能直接通過舊的資產雜湊，須重新驗證程式並更新清單。

安裝Python依賴後，可執行 `python -m unittest discover -s tests -v` 與 `python app.py check-package`，都不連線相機。

獨立的[JPEG工作程式](camera_worker/README.md)使用Zig 0.16.0、靜態musl及固定版本libjpeg-turbo，建置於`camera_worker/`，不屬於以上服務模式模組或安裝資產。它已通過離線主機與模擬器檢查，尚未在相機執行；工作封裝與預設選取仍由主機完成。其建置流程不會安裝或連線相機。

本套件的可攜Python流程尚未在第二台機身驗證；原生工作程式與固定窗口是在一台目標機身完成寫入及完整讀回的版本。離線測試不等於第二台實機測試。
