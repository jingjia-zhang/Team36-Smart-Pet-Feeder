/**

The code below is based on the Doxygen Awesome project, see
https://github.com/jothepro/doxygen-awesome-css

MIT License

Copyright (c) 2021 - 2022 jothepro

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
// Tooltip text shown when hovering over the copy button
let clipboard_title = "Copy to clipboard"
// SVG icon for the default (copy) button
let clipboard_icon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24"><path d="M0 0h24v24H0V0z" fill="none"/><path d="M16 1H4c-1.1 0-2 .9-2 2v14h2V3h12V1zm3 4H8c-1.1 0-2 .9-2 2v14c0 1.1.9 2 2 2h11c1.1 0 2-.9 2-2V7c0-1.1-.9-2-2-2zm0 16H8V7h11v14z"/></svg>`
// SVG icon shown after a successful copy
let clipboard_successIcon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24"><path d="M0 0h24v24H0V0z" fill="none"/><path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41L9 16.17z"/></svg>`
// Duration (in milliseconds) to show the success icon
let clipboard_successDuration = 1000
// Main logic to attach copy buttons (runs after the page loads)
$(function() {
  if(navigator.clipboard) {
    const fragments = document.getElementsByClassName("fragment")// Find all code blocks
    for(const fragment of fragments) {
      const clipboard_div = document.createElement("div")// Create a button container
      clipboard_div.classList.add("clipboard")
      clipboard_div.innerHTML = clipboard_icon
      clipboard_div.title = clipboard_title
      // Click event: copy the code block's content
      $(clipboard_div).click(function() {
        const content = this.parentNode.cloneNode(true)
        // filter out line number and folded fragments from file listings
        content.querySelectorAll(".lineno, .ttc, .foldclosed").forEach((node) => { node.remove() })
        let text = content.textContent
        // remove trailing newlines and trailing spaces from empty lines
        text = text.replace(/^\s*\n/gm,'\n').replace(/\n*$/,'')
        navigator.clipboard.writeText(text);
        this.classList.add("success")
        this.innerHTML = clipboard_successIcon
        window.setTimeout(() => { // switch back to normal icon after timeout
            this.classList.remove("success")
            this.innerHTML = clipboard_icon
        }, clipboard_successDuration);
      })
      // Insert the copy button into the code block
      fragment.insertBefore(clipboard_div, fragment.firstChild)
    }
  }
})
