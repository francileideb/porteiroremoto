const loginForm = document.getElementById("login-form");
const loginButton = document.getElementById("login-form-submit");
const loginErrorMsg = document.getElementById("login-error-msg");

loginButton.addEventListener("click", (e) => {
    e.preventDefault();
    const username = loginForm.username.value;
    const password = loginForm.password.value;

    const xhr = new XMLHttpRequest();
    xhr.open('GET', 'http://172.29.58.40:12913/login?uname='+username+'&pass='+password);
    xhr.onload = () =>{
        // const data = JSON.parse(xhr.response);
        console.log(xhr.response);
        if (xhr.response != "-1") {
        // alert("You have successfully logged in.");
        localStorage.setItem("id", xhr.response);
        location.replace("porteiro.html");
    } else {
        loginErrorMsg.style.opacity = 1;
    }
    };
    xhr.send();
})

