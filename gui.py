"""Small Windows front end. Worker commands remain independently reviewable."""
import os
from pathlib import Path
import queue
import subprocess
import sys
import threading
import tkinter as tk
from tkinter import filedialog,messagebox,ttk

BASE=Path(__file__).resolve().parent

def main():
    root=tk.Tk();root.title('A7C II · FL2 / FL3 預覽版');root.geometry('780x620')
    root.minsize(720,540);q=queue.Queue();running=[False]
    work=tk.StringVar();backup=tk.StringVar();battery=tk.StringVar()
    wrap=ttk.Frame(root,padding=22);wrap.pack(fill='both',expand=True)
    ttk.Label(wrap,text='A7C II · FL2 / FL3',font=('Microsoft JhengHei UI',22,'bold')).pack(anchor='w')
    ttk.Label(wrap,text='僅限原版 2.01｜實驗預覽版｜一台機身通過基本拍攝及回放',wraplength=700).pack(anchor='w',pady=(5,14))
    ttk.Label(wrap,text='先備份與檢查，完成後才可安裝。其他版本與已修改的程式會被拒絕。',wraplength=700).pack(anchor='w',pady=(0,12))
    controls=[]
    for title,var in [('機身工作目錄（首次請選空白資料夾）',work),('第二份備份（建議選另一顆磁碟的空白資料夾）',backup)]:
        ttk.Label(wrap,text=title).pack(anchor='w')
        row=ttk.Frame(wrap);row.pack(fill='x',pady=(3,9))
        entry=ttk.Entry(row,textvariable=var);entry.pack(side='left',fill='x',expand=True)
        def choose(v=var):
            path=filedialog.askdirectory()
            if path:v.set(path)
        button=ttk.Button(row,text='選擇',command=choose);button.pack(side='left',padx=(8,0));controls.extend([entry,button])
    row=ttk.Frame(wrap);row.pack(fill='x',pady=(0,12))
    ttk.Label(row,text='相機目前電量：').pack(side='left')
    entry=ttk.Entry(row,width=7,textvariable=battery);entry.pack(side='left')
    ttk.Label(row,text='%   寫入及回復至少需 50%，期間保持 USB 連接。').pack(side='left');controls.append(entry)
    status=tk.StringVar(value='請先參閱說明，確認兩個 USB 身份的驅動已就緒。')
    ttk.Label(wrap,textvariable=status,wraplength=700).pack(anchor='w',pady=(0,8))
    progress=ttk.Progressbar(wrap,mode='indeterminate');progress.pack(fill='x')
    buttons=ttk.Frame(wrap);buttons.pack(fill='x',pady=12)
    log=tk.Text(wrap,height=13,wrap='word',state='disabled',font=('Microsoft JhengHei UI',10));log.pack(fill='both',expand=True)
    def run(command):
        if running[0]:return
        if not work.get():messagebox.showinfo('選擇工作目錄','請先選擇機身工作目錄。');return
        cmd=[sys.executable,'-u',str(BASE/'app.py'),command,'--workspace',work.get()]
        if command=='prepare':
            if not backup.get():messagebox.showinfo('選擇備份目錄','請先選擇第二份備份目錄。');return
            cmd+=['--backup-copy',backup.get()]
        if command in ['install','restore']:
            try:value=int(battery.get())
            except ValueError:value=-1
            if not 50<=value<=100:messagebox.showinfo('確認電量','請輸入實際電量，至少需 50%。');return
            cmd+=['--battery-percent',str(value)]
        running[0]=True;progress.start(12);status.set('處理中，請勿拔線、關機或關閉視窗。')
        for c in controls:c.configure(state='disabled')
        def worker():
            try:
                env=dict(os.environ,PYTHONIOENCODING='utf-8')
                flags=subprocess.CREATE_NO_WINDOW if os.name=='nt' else 0
                with subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,
                                      encoding='utf-8',errors='replace',env=env,cwd=BASE,creationflags=flags) as process:
                    for line in process.stdout:q.put(('log',line))
                    q.put(('done',process.wait()))
            except Exception as exc:q.put(('log',str(exc)+'\n'));q.put(('done',1))
        threading.Thread(target=worker,daemon=True).start()
    for title,command in [('1 備份與檢查','prepare'),('2 安裝預覽版','install'),('3 重開後核對','verify'),('回復原版','restore')]:
        button=ttk.Button(buttons,text=title,command=lambda c=command:run(c));button.pack(side='left',padx=(0,7));controls.append(button)
    def tick():
        try:
            while True:
                kind,value=q.get_nowait()
                if kind=='log':log.configure(state='normal');log.insert('end',value);log.see('end');log.configure(state='disabled')
                else:
                    running[0]=False;progress.stop()
                    for c in controls:c.configure(state='normal')
                    status.set('處理完成，請閱讀下方結果與下一步。' if value==0 else '已停止。請保留工作目錄，依錯誤內容確認狀態；勿盲目重跑寫入。')
        except queue.Empty:pass
        root.after(100,tick)
    def close():
        if running[0]:messagebox.showinfo('尚在處理','讀寫程序尚未結束，請等待結果。')
        else:root.destroy()
    root.protocol('WM_DELETE_WINDOW',close);tick();root.mainloop()
if __name__=='__main__':main()
