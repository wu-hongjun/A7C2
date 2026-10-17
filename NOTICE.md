# 來源及授權

原生服務通訊使用 ma1co 的 Sony-PMCA-RE，固定提交 `a82f5baaa8e9c3d9f28f94699e860fb2e48cc8e0`。附帶Python來源位於 `vendor/pmca`，MIT授權全文見 `licenses/Sony-PMCA-RE-MIT.txt`。

研究方法與對照資料：

- [lzh2016zzz/a7cr-fl23-research](https://github.com/lzh2016zzz/a7cr-fl23-research/tree/43ba74441c8f10459ade41e84a379ad8005e6ef4)：A7CR FL2／FL3 分層移植與參數、標籤、WBI 的研究參考。本工具為 A7C II 另行定位及實作，沒有把A7CR偏移直接刷入。
- [YuuLuo/a7c2-service-preload](https://github.com/YuuLuo/a7c2-service-preload/tree/1619f938663a36d41e72302a8cfee89a0f6acd1a)：A7C II 2.01 服務模式研究參考，文件為 CC BY-SA 4.0。本工具採原生 `libJit.so` 載入入口，沒有使用預載配置或會改時鐘的 DEBUG 命令。
- Sony官方 A7C II 2.01 與 Alpha 1 II 4.00 是程式／色彩資料分析來源；Sony商標與韌體資料權利仍屬原權利人。本包沒有完整Sony韌體、官方更新容器、任一機身的完整備份或校準單元。新程式碼的MIT授權不宣稱涵蓋Sony的韌體資料。

這是非Sony官方的研究預覽版；發布內容與驗證狀態以對應倉庫說明及 `VALIDATION.json` 為準。
