const abrirForm = document.getElementById("abrir-form");
const sairForm = document.getElementById("sair-form");
const abrirButton = document.getElementById("abrir-submit");
const sairButton = document.getElementById("sair-submit");
const abrirErrorMsg = document.getElementById("open-error-msg");

abrirButton.addEventListener("click", (e) => {
    e.preventDefault();

    const xhr = new XMLHttpRequest();
    xhr.open('GET', 'http://192.168.0.178:12913/abrir?id=');
    xhr.onload = () =>{
        // const data = JSON.parse(xhr.response);
        console.log(xhr.response);
        if (xhr.response != "-1") {
        // alert("You have successfully logged in.");
        location.reload();
    } else {
        loginErrorMsg.style.opacity = 1;
    }
    };
    xhr.send();
})

sairButton.addEventListener("click", (e) => {
    e.preventDefault();

    location.replace("http://192.168.0.178");
})